import { Injectable } from '@angular/core';
import {Observable} from "rxjs/Observable";
import {HttpClient} from "@angular/common/http";
import {AppSettings} from "./AppSettings";
import {DeviceDetails} from "./device-details";

@Injectable()
export class DeviceService {
  url = AppSettings.API_ENDPOINT;

  constructor(private httpClient: HttpClient) { }

  activateDevice(deviceKey: string): Observable<object> {
    return this.httpClient.put(this.url + '/devices/' + deviceKey + '/activate', {})
  }

  getDeviceDetails(deviceKey: string): Observable<object> {
    return this.httpClient.get(this.url + '/devices/' + deviceKey + '/details');
  }

  updateDeviceDetails(deviceKey: string, details: DeviceDetails) {
    return this.httpClient.put(this.url + '/devices/' + deviceKey + '/details', details);
  }

}
