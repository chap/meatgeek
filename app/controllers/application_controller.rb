class ApplicationController < ActionController::Base
  protect_from_forgery

  private
  def authenticate
    authenticate_or_request_with_http_token do |token, options|
      token == ENV['API_TOKEN']
    end
  end
end
