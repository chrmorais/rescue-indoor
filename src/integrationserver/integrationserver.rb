#!/usr/bin/ruby -w

require 'gserver'
require './rescueindoormodel.rb'

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
