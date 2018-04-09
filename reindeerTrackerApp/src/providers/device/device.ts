import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {Observable} from "rxjs/Observable";
import {AppSettings} from "../../AppSettings";
import {DeviceDetails} from "../../classes/deviceDetails";
import {AuthProvider} from "../auth/auth";

@Injectable()
export class DeviceProvider {
  api = AppSettings.API_ENDPOINT;

  constructor(public http: HttpClient, private auth: AuthProvider) {

  }

  getDevice(deviceKey: string): Observable<any> {
    return this.http.get(this.api + '/devices/' + deviceKey);
  }

  getDeviceDetails(deviceKey: string): Observable<any> {
    return this.http.get(this.api + '/devices/' + deviceKey + '/details');
  }

  getUserDevices(): Observable<any> {
    const uid = this.auth.getCurrentUID();
    return this.http.get(this.api + '/users/' + uid + '/devices');
  }

  addDeviceToUser(deviceKey: string): Observable<any> {
    const uid = this.auth.getCurrentUID();
    return this.http.put(this.api + '/users/' + uid + '/devices', {deviceKey: deviceKey});
  }

  removeDeviceFromUser(deviceKey: string): Observable<any> {
    const uid = this.auth.getCurrentUID();
    return this.http.delete(this.api + '/users/' + uid + '/devices?deviceKey=' + deviceKey);
  }

  setDeviceDetails(deviceKey: string, details: DeviceDetails): Observable<any> {
    return this.http.put(this.api + '/devices/' + deviceKey + '/details', details);
  }

}
