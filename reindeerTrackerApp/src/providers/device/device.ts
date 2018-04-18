import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {Observable} from "rxjs/Observable";
import {AppSettings} from "../../AppSettings";
import {DeviceDetails} from "../../classes/deviceDetails";
import {AuthProvider} from "../auth/auth";
import {Device} from "../../classes/device";

@Injectable()
export class DeviceProvider {
  api = AppSettings.API_ENDPOINT;
  private devices: Device[] = [];

  constructor(public http: HttpClient, private auth: AuthProvider) {

  }

  loadUserDevices(): Promise<any> {
    return new Promise((resolve, reject) => {
      this.getUserDevices()
        .subscribe(devices => {
          this.devices = devices;
          resolve(devices)
        }, err => {
          reject();
        })
    });
  }

  getUserDevicesFromStorage(): Promise<any> {
    return new Promise((resolve, reject) => {
      if (this.devices.length > 0) {
        resolve(this.getDevicesViaValue());
      } else {
        this.loadUserDevices()
          .then(() => {
            resolve(this.getDevicesViaValue());
          });
      }
    });
  }

  private getDevicesViaValue(): any {
    return JSON.parse(JSON.stringify(this.devices));
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
