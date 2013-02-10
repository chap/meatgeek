require 'test_helper'

class ReadingsControllerTest < ActionController::TestCase
  setup do
    @reading = readings(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:readings)
  end

  test "should create reading" do
    assert_difference('Reading.count', 2) do
      post :create, format: 'json', readings: [{probe_id: 1, temperature: 100 }, {probe_id: 2, temperature: 200}]
    end

    assert response.body.include?(100)
    assert response.body.include?(200)
  end
end
