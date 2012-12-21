class AddProbeIdToReadings < ActiveRecord::Migration
  def change
    add_column :readings, :probe_id, :integer
  end
end
