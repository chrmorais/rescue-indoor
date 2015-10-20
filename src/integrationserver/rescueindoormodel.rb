require 'active_record'

ActiveRecord::Base.logger = Logger.new(File.open('database.log', 'w'))

ActiveRecord::Base.establish_connection(
  :adapter  => 'sqlite3',
  :database => 'rescueindoor.db'
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
