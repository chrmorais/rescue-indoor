#!/usr/bin/ruby -w

require 'sinatra'
require 'json'
require './rescueindoormodel.rb'

# list all users
get '/users' do
  User.all.to_json
end

# view one user
get '/users/:id' do
  user = User.find(params[:id])
  return status 404 if user.nil?
  user.to_json
end

# list all queries
get '/queries' do
  starttime = params[:start]
  endtime = params[:end]
  if !starttime.nil? and !endtime.nil?
    Query.where(:created_at => starttime.to_datetime..endtime.to_datetime).to_json
  else
    if !starttime.nil?
      Query.where("created_at >= ?", starttime.to_datetime).to_json
    else
      if !endtime.nil?
        Query.where("created_at <= ?", endtime.to_datetime).to_json
      else
        Query.all.to_json
      end
    end
  end
end

