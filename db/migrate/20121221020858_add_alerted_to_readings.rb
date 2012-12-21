class AddAlertedToReadings < ActiveRecord::Migration
  def change
    add_column :readings, :alerted, :boolean, :null => false, :default => false
  end
end
