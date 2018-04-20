import { Injectable } from '@angular/core';
import {Observable} from "rxjs/Observable";
import {HttpClient} from "@angular/common/http";
import {AppSettings} from "./AppSettings";
import {DeviceDetails} from "./device-details";
import {AuthService} from "./auth.service";
import {Device} from "./classes/device";
import {reject} from "q";

@Injectable()
export class DeviceService {
  url = AppSettings.API_ENDPOINT;
  devices: Device[] = null;

  constructor(private httpClient: HttpClient, private auth: AuthService) { }

  getUserDevices(): Promise<Device[]> {
    return new Promise<Device[]>((resolve, reject) => {
      if (this.devices) {
        resolve(this.devices);
      }else {
        this.refreshUserDevices()
          .then(() => {
            resolve(this.devices);
          })
      }
    })
  }

  refreshUserDevices(): Promise<Device[]> {
    return new Promise<Device[]>((resolve, reject) => {
      const uid = this.auth.getCurrentUID();
      console.log('making http call');
      this.httpClient.get(this.url + '/users/' + uid + '/devices')
        .subscribe((devices: Device[]) => {
          this.devices = devices;
          resolve(devices);
        }, err => {
          console.log(err);
          reject(err);
        })
    })
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
