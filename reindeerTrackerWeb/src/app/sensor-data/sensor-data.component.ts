import {Component, OnInit, ViewChild} from '@angular/core';
import { SensorDataService } from './sensor-data.service';
import { HttpClient } from 'selenium-webdriver/http';
import { element } from 'protractor';
import {AuthService} from "../shared/auth.service";
import {DeviceService} from "../shared/device.service";
import {Router} from "@angular/router";
import {Device} from "../shared/classes/device";
import {AliveState, BatteryState, FilterOptions} from "../shared/classes/filteroptions";
import {KeysService} from "../shared/keys.service";

@Component({
  selector: 'app-sensor-data',
  templateUrl: './sensor-data.component.html',
  styleUrls: ['./sensor-data.component.css'],
  providers: [SensorDataService]
})
export class SensorDataComponent implements OnInit {
  showDeleteModal: boolean = false;
  deviceToDelete: string = '';
  showNewDeviceModal: boolean = false;
  newDevice: string = '';
  errorMessage: string = '';
  displayingDevices: Device[] = [];
  devices: Device[] = [];
  filterOptions: FilterOptions;
  aliveState = AliveState;
  batteryState = BatteryState;

  constructor(private server: SensorDataService, private deviceService: DeviceService, private router: Router, public authService: AuthService, private keyService: KeysService) {
    this.filterOptions = {
      alive: AliveState.all,
      battery: BatteryState.all
    }
  }

  getDevices() {
    this.deviceService.getUserDevices()
      .subscribe((devices: Device[]) => {
        console.log(devices);
        this.devices = devices;
        this.resetList();
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

  editDevice(deviceKey: string) {
    this.router.navigate(['/device-info'], {queryParams: {deviceKey: deviceKey}});
  }

  openDeleteModal(deviceKey: string) {
    this.deviceToDelete = deviceKey;
    this.showDeleteModal = true;
  }

  openNewDeviceModal() {
    this.showNewDeviceModal = true;
  }

  closeModal() {
    this.showDeleteModal = false;
    this.showNewDeviceModal = false;
    this.newDevice = '';
  }

  removeDevice(deviceKey: string) {
    this.deviceService.removeDevice(deviceKey)
      .subscribe(res => {
        this.getDevices();
        this.closeModal();
      }, err => {
        console.log(err);
      })
  }

  addFilter() {
    /*console.log(this.filterOptions.battery);
    if (this.filterOptions.alive == AliveState.all && this.filterOptions.battery == BatteryState.all) {
      console.log('reset filter');
      this.resetList();
    } else {
      console.log('applying filter');
      this.displayingDevices = [];
      if (this.filterOptions.alive == AliveState.alive) {
        for (let i=0; i<this.devices.length; i++) {
          if (this.devices[i].isAlive) {
            console.log('adding: ' + this.devices[i]);
            this.displayingDevices.push(this.devices[i]);
          }
        }
      } else if (this.filterOptions.alive == AliveState.dead) {
        for (let i=0; i<this.devices.length; i++) {
          if (!this.devices[i].isAlive) {
            this.displayingDevices.push(this.devices[i]);
          }
        }
      } else {
        console.log('reset');
        this.resetList();
      }
      console.log(this.displayingDevices);
      console.log(this.devices);
      if (this.filterOptions.battery != BatteryState.all) {
        for (let i=0; i<this.displayingDevices.length; i++) {
          console.log(this.displayingDevices[i].lastLog.battery, 'battery: ' + this.displayingDevices[i].deviceKey);
          if (this.filterOptions.battery == BatteryState.high && this.displayingDevices[i].lastLog.battery <= 20) {
            this.displayingDevices.splice(i, 1);
            console.log('removing: ' + this.displayingDevices[i].deviceKey);
          } else if (this.filterOptions.battery == BatteryState.low && this.displayingDevices[i].lastLog.battery > 20) {
            this.displayingDevices.splice(i, 1);
            console.log('removing: ' + this.displayingDevices[i].deviceKey);
          }
        }
      }
      console.log(this.displayingDevices);
    }*/
  }

  resetList() {
    this.displayingDevices = this.devices.slice();
  }

  public addKey() {
    this.errorMessage = '';
    if (this.newDevice.length != 6) {
      this.errorMessage = 'Device key is not valid';
    } else {
      this.keyService.addKeyToUser(this.authService.getCurrentUID(), this.newDevice)
        .subscribe(res => {
            console.log(res);
            this.router.navigate(['device-info'], {queryParams: {deviceKey: this.newDevice}});
          },
          err => {
            console.log(err);
            this.errorMessage = 'A device with the key ' + this.newDevice + ' does not exist';
          });
    }
  }
}
