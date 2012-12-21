class Reading < ActiveRecord::Base
  attr_accessible :temperature, :probe_id

  validates :temperature, :probe_id, :presence => true

  def meat?
    probe_id == 1
  end

  def air?
    probe_id == 2
  end

  def self.alert_air_temp
    alarming_reading = Reading.where(:probe_id => 2).order("created_at desc").limit(1).first
    if alarming_reading.temperature < ENV['ALERT_AIR_TEMP'].to_i
      Rails.logger.info("[ALERT] - LOW AIR TEMP!")
      # only call if haven't called in 10 minutes
      if Reading.where(:probe_id => 2).where(:alerted => true).where("created_at >= ?", 10.minutes.ago).limit(1).first
        Rails.logger.info("[ALERT] - skipping call, been alerted in last 10 min")
      else
        alarming_reading.update_attribute(:alerted, true)
        call = TWILIO_ACCOUNT.calls.create(
          :from => ENV['TWILIO_FROM_NUMBER'],
          :to => ENV['TWILIO_TO_NUMBER'],
          :url => "http://twimlets.com/message?Message%5B0%5D=Dude%2C%20wake%20up.%20Air%20temperature%20is%20low!"
        )
        
        Rails.logger.info("[ALERT] - #{call.inspect}")
      end
    end
  end
end
