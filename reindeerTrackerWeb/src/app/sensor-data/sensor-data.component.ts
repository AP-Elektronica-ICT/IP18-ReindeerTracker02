import { Component, OnInit } from '@angular/core';
import { SensorDataService } from './sensor-data.service';
import { HttpClient } from 'selenium-webdriver/http';
import { element } from 'protractor';
import {AuthService} from "../shared/auth.service";
import {DeviceService} from "../shared/device.service";
import {Router} from "@angular/router";
import {Device} from "../shared/classes/device";

@Component({
  selector: 'app-sensor-data',
  templateUrl: './sensor-data.component.html',
  styleUrls: ['./sensor-data.component.css'],
  providers: [SensorDataService]
})
export class SensorDataComponent implements OnInit {
  naam : string;
  devices: Device[] = [];

  constructor(private server: SensorDataService, private deviceService: DeviceService, private router: Router) { }

  getDevices() {
    this.deviceService.getUserDevices()
      .subscribe((devices: Device[]) => {
        console.log(devices);
        this.devices = devices;
      });
  }

  selectDevice(deviceKey: string) {
    this.router.navigate(['/detail'], {queryParams: {deviceKey: deviceKey}});
  }

  ngOnInit() {
    this.getDevices();
  }

  getAliveIcon(device: Device) {
    if (device.isAlive) {
      return 'fas fa-heart'
    } else {
      return 'far fa-heart'
    }
  }

  getAliveText(device: Device) {
    if (device.isAlive) {
      return 'alive'
    } else {
      return 'dead'
    }
  }

  getBatteryIcon(device: Device) {
    if (device.lastLog.battery > 75) {
      return 'fas fa-battery-full';
    } else if (device.lastLog.battery > 50) {
      return 'fas fa-battery-three-quarters';
    } else if (device.lastLog.battery > 25) {
      return 'fas fa-battery-half';
    } else if (device.lastLog.battery > 0) {
      return 'fas fa-battery-quarter red';
    } else {
      return 'fas fa-battery-empty red';
    }
  }
}
