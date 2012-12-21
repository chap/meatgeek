namespace :alerts do
  desc 'call me if no readings in 10 minutes'
  task :no_readings  => :environment do
    unless Reading.where('created_at >= ?', 10.minutes.ago).limit(1).present?
      # no reading in 10 minutes? sound the alarms
      call = TWILIO_ACCOUNT.calls.create(
        :from => ENV['TWILIO_FROM_NUMBER'],
        :to => ENV['TWILIO_TO_NUMBER'],
        :url => "http://twimlets.com/message?Message%5B0%5D=Dude%2C%20wake%20up.%20No%20readings%20in%20over%2010%20minutes!"
      )
      Rails.logger.info("[ALERT] - OVER 10 MINUTES SINCE LAST READING!")
      Rails.logger.info("[ALERT] - #{call.inspect}")
    end
  end
end