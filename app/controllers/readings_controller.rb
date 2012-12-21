class ReadingsController < ApplicationController
  before_filter :authenticate, :only => [:create]

  # GET /readings
  # GET /readings.json
  def index
    @readings = Reading.order('created_at desc')
    @readings_by_minute = @readings.group_by {|r| r.created_at.strftime("%D %H:%M") }

    respond_to do |format|
      format.html # index.html.erb
      format.json { render json: @readings }
    end
  end

  # GET /readings/1
  # GET /readings/1.json
  def show
    @reading = Reading.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.json { render json: @reading }
    end
  end

  # GET /readings/new
  # GET /readings/new.json
  def new
    @reading = Reading.new

    respond_to do |format|
      format.html # new.html.erb
      format.json { render json: @reading }
    end
  end

  # GET /readings/1/edit
  def edit
    @reading = Reading.find(params[:id])
  end

  # POST /readings
  # POST /readings.json
  def create
    @reading1 = Reading.create(params[:readings].first)
    @reading2 = Reading.create(params[:readings].last)

    respond_to do |format|
      if !@reading1.new_record? && !@reading2.new_record?
        Reading.alert_air_temp
        format.json { render json: [@reading1, @reading2], status: :created }
      else
        format.json { render json: [@reading1.errors, @reading2.errors], status: :unprocessable_entity }
      end
    end
  end

  # PUT /readings/1
  # PUT /readings/1.json
  def update
    @reading = Reading.find(params[:id])

    respond_to do |format|
      if @reading.update_attributes(params[:reading])
        format.html { redirect_to @reading, notice: 'Reading was successfully updated.' }
        format.json { head :no_content }
      else
        format.html { render action: "edit" }
        format.json { render json: @reading.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /readings/1
  # DELETE /readings/1.json
  def destroy
    @reading = Reading.find(params[:id])
    @reading.destroy

    respond_to do |format|
      format.html { redirect_to readings_url }
      format.json { head :no_content }
    end
  end
end
