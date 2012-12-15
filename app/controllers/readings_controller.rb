class ReadingsController < ApplicationController
  # GET /readings
  # GET /readings.json
  def index
    @readings = Reading.all

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
    @reading = Reading.new(params[:reading])

    respond_to do |format|
      if @reading.save
        format.html { redirect_to @reading, notice: 'Reading was successfully created.' }
        format.json { render json: @reading, status: :created, location: @reading }
      else
        format.html { render action: "new" }
        format.json { render json: @reading.errors, status: :unprocessable_entity }
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
