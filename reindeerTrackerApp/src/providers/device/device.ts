import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {Observable} from "rxjs/Observable";
import {AppSettings} from "../../AppSettings";
import {DeviceDetails} from "../../classes/deviceDetails";

@Injectable()
export class DeviceProvider {
  api = AppSettings.API_ENDPOINT;

  constructor(public http: HttpClient) {

  }

  getDevice(deviceKey: string): Observable<any> {
    return this.http.get(this.api + '/devices/' + deviceKey);
  }

  getDeviceDetails(deviceKey: string): Observable<any> {
    return this.http.get(this.api + '/devices/' + deviceKey + '/details');
  }

  getUserDevices(): Observable<any> {
    //TODO: get user id from firebase login
    const uid = 'DDQ4cy9jMeYkVpHvqVoiUNJ76GI3';
    return this.http.get(this.api + '/users/' + uid + '/devices');
  }

  addDeviceToUser(deviceKey: string): Observable<any> {
    //TODO: get user id from firebase login
    const uid = 'DDQ4cy9jMeYkVpHvqVoiUNJ76GI3';
    return this.http.put(this.api + '/users/' + uid + '/devices', {deviceKey: deviceKey});
  }

  setDeviceDetails(deviceKey: string, details: DeviceDetails): Observable<any> {
    return this.http.put(this.api + '/devices/' + deviceKey + '/details', details);
  }

}
