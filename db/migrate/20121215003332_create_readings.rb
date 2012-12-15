class CreateReadings < ActiveRecord::Migration
  def change
    create_table :readings do |t|
      t.decimal :temperature, :precision => 8, :scale => 2

      t.timestamps
    end
  end
end
