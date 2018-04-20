import { Injectable } from '@angular/core';
import {Observable} from "rxjs/Observable";
import {HttpClient} from "@angular/common/http";
import {AppSettings} from "./AppSettings";
import {DeviceDetails} from "./device-details";
import {AuthService} from "./auth.service";

@Injectable()
export class DeviceService {
  url = AppSettings.API_ENDPOINT;

  constructor(private httpClient: HttpClient, private auth: AuthService) { }

  getUserDevices(): Observable<object> {
    const uid = this.auth.getCurrentUID();
    return this.httpClient.get(this.url + '/users/' + uid + '/devices');
  }

  activateDevice(deviceKey: string): Observable<object> {
    return this.httpClient.put(this.url + '/devices/' + deviceKey + '/activate', {})
  }

  getDeviceFull(deviceKey: string): Observable<object> {
    return this.httpClient.get(this.url + '/devices/' + deviceKey);
  }

  getDeviceDetails(deviceKey: string): Observable<object> {
    return this.httpClient.get(this.url + '/devices/' + deviceKey + '/details');
  }

  updateDeviceDetails(deviceKey: string, details: DeviceDetails) {
    return this.httpClient.put(this.url + '/devices/' + deviceKey + '/details', details);
  }

  removeDevice(deviceKey: string) {
    const uid = this.auth.getCurrentUID();
    return this.httpClient.delete(this.url + '/users/' + uid + '/devices?deviceKey=' + deviceKey);
  }

}
