#!/usr/bin/ruby -w

require 'gserver'
require 'active_record'

ActiveRecord::Base.logger = Logger.new(File.open('database.log', 'w'))

ActiveRecord::Base.establish_connection(
  :adapter  => 'sqlite3',
  :database => 'rescue-indoor.db'
)

ActiveRecord::Schema.define do
  unless ActiveRecord::Base.connection.tables.include? 'users'
    create_table :users do |table|
      table.column :rfid,      :string
      table.column :full_name, :string
    end
  end
end

ActiveRecord::Schema.define do
  unless ActiveRecord::Base.connection.tables.include? 'queries'
    create_table :queries do |table|
      table.column :hostname,   :string
      table.column :rfid,       :string
      table.column :created_at, :datetime
    end
  end
end

class User < ActiveRecord::Base
end

class Query < ActiveRecord::Base
end

class ChatServer < GServer
  def initialize(port=20606, host=GServer::DEFAULT_HOST)
    super(port, host, Float::MAX, $stderr, true)
  end

  def serve(sock)
    until sock.eof? do
      message = sock.gets.chomp
      puts message
      user = User.find_by_rfid(message)
      if user.nil?
        sock.puts "<user-not-found>"
      else
        sock.puts user.full_name
      end
      hostname = sock.peeraddr[2] || sock.peeraddr[3]
      Query.create(:hostname => hostname, :rfid => message)
    end
  end
end

server = ChatServer.new(*ARGV[0..2] || 20606)
server.start(-1)
server.join
