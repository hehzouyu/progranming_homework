#include <iostream>
#include <cmath>
#include <string>
#include <iomanip>

using namespace std;


enum class CarType { SMALL = 1, LARGE = 2 };
enum class SpotType { NORMAL = 1, CHARGING = 2 };
enum class VehicleType { NEW_ENERGY = 1, NORMAL = 2 };
enum class ChargingStatus { NOT_CHARGING = 0, CHARGING = 1, FULL = 2 };

class ParkingRecord {
private:
    CarType carType;
    SpotType spotType;
    VehicleType vehicleType;
    bool isCharging;
    ChargingStatus chargeStatus;
    int minutes;
    int overdueMinutes;

public:
  
    ParkingRecord(int ct, int st, int vt, int ic, int cs, int min, int om) 
        : carType(static_cast<CarType>(ct)), 
          spotType(static_cast<SpotType>(st)),
          vehicleType(static_cast<VehicleType>(vt)),
          isCharging(ic == 1),
          chargeStatus(static_cast<ChargingStatus>(cs)),
          minutes(min),
          overdueMinutes(om) {}
    
  
    CarType getCarType() const { return carType; }
    SpotType getSpotType() const { return spotType; }
    VehicleType getVehicleType() const { return vehicleType; }
    bool getIsCharging() const { return isCharging; }
    ChargingStatus getChargeStatus() const { return chargeStatus; }
    int getMinutes() const { return minutes; }
    int getOverdueMinutes() const { return overdueMinutes; }
   
    string toString() const {
        stringstream ss;
        ss << "车型:" << (carType == CarType::SMALL ? "小车" : "大车")
           << " 车位:" << (spotType == SpotType::NORMAL ? "普通" : "充电")
           << " 车辆类型:" << (vehicleType == VehicleType::NEW_ENERGY ? "新能源" : "普通")
           << " 充电状态:" << (isCharging ? (chargeStatus == ChargingStatus::CHARGING ? "充电中" : "已充满") : "未充电")
           << " 停车时长:" << minutes << "分钟";
        if (chargeStatus == ChargingStatus::FULL) {
            ss << " 充满后:" << overdueMinutes << "分钟";
        }
        return ss.str();
    }
};

class FeeResult {
private:
    double parkingFee;
    double occupancyFee;
    
public:
    FeeResult(double pf = 0.0, double of = 0.0) : parkingFee(pf), occupancyFee(of) {}
    
    double getParkingFee() const { return parkingFee; }
    double getOccupancyFee() const { return occupancyFee; }
    double getTotal() const { return parkingFee + occupancyFee; }
    
    void setParkingFee(double fee) { parkingFee = fee; }
    void setOccupancyFee(double fee) { occupancyFee = fee; }
    
    string toString() const {
        stringstream ss;
        ss << fixed << setprecision(2);
        ss << "停车费：" << parkingFee << "元，占位费：" << occupancyFee << "元\n";
        ss << "合计收费：" << getTotal() << "元";
        return ss.str();
    }
};

class ParkingFeeCalculator {
private:
    
    static constexpr int FREE_MINUTES = 30;
    static constexpr int DAY_MINUTES = 24 * 60;
    static constexpr double OCCUPANCY_CAP = 100.0;
    
    
    struct RateConfig {
        double normalSpotSmall;      
        double normalSpotLarge;      
        double normalSpotCapSmall;  
        double normalSpotCapLarge;   
        
        double occupySpotSmallFirst; 
        double occupySpotLargeFirst; 
        double occupySpotSmallAfter; 
        double occupySpotLargeAfter; 
        double occupySpotCapSmall;   
        double occupySpotCapLarge;   
        
        double occupancyPerMinute;   
    };
    
    RateConfig rates;
    
    
    int calculateHours(int minutes) const {
        if (minutes <= 0) return 0;
        return (minutes + 59) / 60;  
    }
    
    
    double applyDailyCap(double fee, int minutes, double cap) const {
        if (minutes > DAY_MINUTES) {
            return cap;
        }
        return min(fee, cap);
    }

public:
    ParkingFeeCalculator() {
        
        rates = {
            5.0, 10.0, 50.0, 100.0,           
            15.0, 30.0, 30.0, 60.0, 150.0, 300.0, 
            1.0                               
        };
    }
    
   
    FeeResult calculate(const ParkingRecord& record) const {
        double parkingFee = 0.0;
        double occupancyFee = 0.0;
        
        
        if (record.getSpotType() == SpotType::NORMAL) {
            
            auto [pf, of] = calculateNormalSpot(record);
            parkingFee = pf;
            occupancyFee = of;
        } else {
            
            auto [pf, of] = calculateChargingSpot(record);
            parkingFee = pf;
            occupancyFee = of;
        }
        
        return FeeResult(parkingFee, occupancyFee);
    }
    
private:
    
    pair<double, double> calculateNormalSpot(const ParkingRecord& record) const {
        double parkingFee = 0.0;
        int minutes = record.getMinutes();
        
        if (minutes > FREE_MINUTES) {
            int hours = calculateHours(minutes);
            
            if (record.getCarType() == CarType::SMALL) {
                parkingFee = hours * rates.normalSpotSmall;
                parkingFee = applyDailyCap(parkingFee, minutes, rates.normalSpotCapSmall);
            } else {
                parkingFee = hours * rates.normalSpotLarge;
                parkingFee = applyDailyCap(parkingFee, minutes, rates.normalSpotCapLarge);
            }
        }
        
        return {parkingFee, 0.0};
    }
    
    
    pair<double, double> calculateChargingSpot(const ParkingRecord& record) const {
        
        if (record.getVehicleType() == VehicleType::NEW_ENERGY && 
            record.getIsCharging()) {
            
            if (record.getChargeStatus() == ChargingStatus::CHARGING) {
                
                return {0.0, 0.0};
            } else if (record.getChargeStatus() == ChargingStatus::FULL) {
                
                double occupancyFee = record.getOverdueMinutes() * rates.occupancyPerMinute;
                occupancyFee = min(occupancyFee, OCCUPANCY_CAP);
                return {0.0, occupancyFee};
            }
        }
        
        
        return calculateOccupancyFee(record);
    }
    
    
    pair<double, double> calculateOccupancyFee(const ParkingRecord& record) const {
        double parkingFee = 0.0;
        int minutes = record.getMinutes();
        int hours = calculateHours(minutes);
        
        if (record.getCarType() == CarType::SMALL) {
         
            if (minutes <= FREE_MINUTES) {
                parkingFee = hours * rates.occupySpotSmallFirst;
            } else {
                parkingFee = hours * rates.occupySpotSmallAfter;
            }
            parkingFee = applyDailyCap(parkingFee, minutes, rates.occupySpotCapSmall);
        } else {
           
            if (minutes <= FREE_MINUTES) {
                parkingFee = hours * rates.occupySpotLargeFirst;
            } else {
                parkingFee = hours * rates.occupySpotLargeAfter;
            }
            parkingFee = applyDailyCap(parkingFee, minutes, rates.occupySpotCapLarge);
        }
        
        return {parkingFee, 0.0};
    }
};


class TestFramework {
public:
    static void runTest(const string& testName, const ParkingRecord& record, 
                       double expectedParking, double expectedOccupancy) {
        ParkingFeeCalculator calculator;
        FeeResult result = calculator.calculate(record);
        
        bool pass = (abs(result.getParkingFee() - expectedParking) < 0.01) &&
                   (abs(result.getOccupancyFee() - expectedOccupancy) < 0.01);
        
        cout << "========================================\n";
        cout << "测试用例：" << testName << "\n";
        cout << "输入参数：" << record.toString() << "\n";
        cout << "计算结果：\n" << result.toString() << "\n";
        cout << "预期结果：停车费：" << fixed << setprecision(2) << expectedParking 
             << "元，占位费：" << expectedOccupancy << "元\n";
        cout << "测试状态：" << (pass ? "通过" : "❌ 失败") << "\n\n";
    }
};

int main() {
    cout << "XX充电站停车计费系统 - 面向对象实现\n";
    cout << "=====================================\n\n";
    
    
    TestFramework::runTest("1.普通车位-小车≤30分钟", 
        ParkingRecord(1, 1, 1, 0, 0, 25, 0), 0.0, 0.0);
    
    TestFramework::runTest("2.普通车位-小车>30分钟(90分钟)", 
        ParkingRecord(1, 1, 1, 0, 0, 90, 0), 10.0, 0.0);
    
    TestFramework::runTest("3.普通车位-大车(120分钟)", 
        ParkingRecord(2, 1, 2, 0, 0, 120, 0), 20.0, 0.0);
    
    TestFramework::runTest("4.充电车位-新能源车充电中", 
        ParkingRecord(1, 2, 1, 1, 1, 300, 0), 0.0, 0.0);
    
    TestFramework::runTest("5.充电车位-充满后20分钟", 
        ParkingRecord(1, 2, 1, 1, 2, 240, 20), 0.0, 20.0);
    
    TestFramework::runTest("6.充电车位-充满后45分钟", 
        ParkingRecord(1, 2, 1, 1, 2, 300, 45), 0.0, 45.0);
    
    TestFramework::runTest("7.充电车位-新能源车未充电(按占用处理)", 
        ParkingRecord(1, 2, 1, 0, 0, 45, 0), 30.0, 0.0);
    
    TestFramework::runTest("8.占用充电车位-非新能源车大车>30分钟", 
        ParkingRecord(2, 2, 2, 0, 0, 150, 0), 180.0, 0.0);
    
    TestFramework::runTest("9.封顶测试-占位费超100元", 
        ParkingRecord(1, 2, 1, 1, 2, 600, 110), 0.0, 100.0);
    
    TestFramework::runTest("10.封顶测试-停车费超24小时", 
        ParkingRecord(1, 1, 1, 0, 0, 1500, 0), 50.0, 0.0);
    
    // 交互式输入示例
    cout << "\n========================================\n";
    cout << "交互式计算示例（输入参数）：\n";
    cout << "请输入（车型 车位类型 车辆类型 是否充电 充电状态 停车分钟 充满后分钟）：\n";
    cout << "示例：1 2 1 1 2 300 45\n";
    
    int ct, st, vt, ic, cs, min, om;
    if (cin >> ct >> st >> vt >> ic >> cs >> min >> om) {
        ParkingRecord record(ct, st, vt, ic, cs, min, om);
        ParkingFeeCalculator calculator;
        FeeResult result = calculator.calculate(record);
        
        cout << "\n计算结果：\n";
        cout << result.toString() << endl;
    }
    
    return 0;
}