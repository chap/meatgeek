# set up a client to talk to the Twilio REST API
if ENV['TWILIO_SID'].present?
  client = Twilio::REST::Client.new(ENV['TWILIO_SID'], ENV['TWILIO_TOKEN'])
  TWILIO_ACCOUNT = client.account
end