class ReadingsController < ApplicationController
  before_filter :authenticate, :only => [:create]

  # GET /readings
  # GET /readings.json
  def index
    @readings = Reading.order('created_at desc').limit(100)
    @readings_by_minute = @readings.group_by {|r| r.created_at.strftime("%D %H:%M") }

    respond_to do |format|
      format.html # index.html.erb
      format.json { render json: @readings }
    end
  end

  # POST /readings
  # POST /readings.json
  def create
    @reading1 = Reading.create(params[:readings].first)
    @reading2 = Reading.create(params[:readings].last)

    if !@reading1.new_record? && !@reading2.new_record?
      Reading.alert_air_temp
      render json: [@reading1, @reading2], status: :created
    else
      render json: [@reading1.errors, @reading2.errors], status: :unprocessable_entity
    end
  end
end
