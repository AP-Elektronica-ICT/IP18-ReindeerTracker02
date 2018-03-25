import { Component, OnInit } from '@angular/core';
import { SensorDataService } from './sensor-data.service';
import { HttpClient } from 'selenium-webdriver/http';
import { element } from 'protractor';
import {AuthService} from "../shared/auth.service";
import {DeviceService} from "../shared/device.service";
import {Router} from "@angular/router";

@Component({
  selector: 'app-sensor-data',
  templateUrl: './sensor-data.component.html',
  styleUrls: ['./sensor-data.component.css'],
  providers: [SensorDataService]
})
export class SensorDataComponent implements OnInit {
  naam : string;
  devices = null;

  constructor(private server: SensorDataService, private deviceService: DeviceService, private router: Router) { }

  getDevices() {
    this.deviceService.getUserDevices()
      .subscribe(res => {
        console.log(res);
        this.devices = res;
      });
  }

  selectDevice(deviceKey: string) {
    this.router.navigate(['/detail'], {queryParams: {deviceKey: deviceKey}});
  }

  ngOnInit() {
    this.getDevices();
  }
}
