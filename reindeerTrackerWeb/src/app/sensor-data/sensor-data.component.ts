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
import {InviteService} from "../shared/invite.service";

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
  showInviteModal: boolean = false;
  errorMessage: string = '';
  invite: string = '';
  inviteDeviceKey: string = '';
  inviteErrorMessage: string = '';
  inviteSuccessMessage: string = '';
  displayingDevices: Device[] = [];
  devices: Device[] = [];
  filterOptions: FilterOptions;
  aliveState = AliveState;
  batteryState = BatteryState;

  constructor(private server: SensorDataService, private deviceService: DeviceService, private router: Router, public authService: AuthService, private keyService: KeysService, private inviteService: InviteService) {
    this.filterOptions = {
      alive: AliveState.all,
      battery: BatteryState.all
    }
  }

  getDevices() {
    console.log('getting devices');
    this.deviceService.getUserDevices()
      .then((devices: Device[]) => {
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
    this.errorMessage = '';
    this.showInviteModal = false;
    this.invite = '';
    this.inviteErrorMessage = '';
    this.inviteSuccessMessage = '';
  }

  removeDevice(deviceKey: string) {
    this.deviceService.removeDevice(deviceKey)
      .subscribe(res => {
        this.deviceService.refreshUserDevices()
          .then(() => {
            this.getDevices();
          });
        this.closeModal();
      }, err => {
        console.log(err);
      })
  }

  addFilter() {
    this.displayingDevices = [];
    if (this.filterOptions.alive == AliveState.all && this.filterOptions.battery == BatteryState.all) {
      this.resetList();
    } else {
      let temp: Device[] = [];
      if (this.filterOptions.alive == AliveState.alive) {
        for (let i=0; i<this.devices.length; i++) {
          if (this.devices[i].isAlive) {
            temp.push(this.devices[i]);
          }
        }
      } else if (this.filterOptions.alive == AliveState.dead) {
        for (let i=0; i<this.devices.length; i++) {
          if (!this.devices[i].isAlive) {
            temp.push(this.devices[i]);
          }
        }
      } else {
        console.log('reset');
        temp = this.devices.slice();
      }
      if (this.filterOptions.battery == BatteryState.high) {
        for (let i=0; i<temp.length; i++) {
          if (temp[i].lastLog.battery > 20) {
            this.displayingDevices.push(temp[i]);
          }
        }
      } else if (this.filterOptions.battery == BatteryState.low) {
        for (let i=0; i<temp.length; i++) {
          if (temp[i].lastLog.battery <= 20) {
            this.displayingDevices.push(temp[i]);
          }
        }
      } else {
        this.displayingDevices = temp.slice();
      }
    }
  }

  resetList() {
    this.displayingDevices = this.devices.slice();
  }

  public addKey() {
    this.errorMessage = '';
    if (this.newDevice.length != 5) {
      this.errorMessage = 'Device key is not valid';
    } else {
      this.keyService.addKeyToUser(this.authService.getCurrentUID(), this.newDevice)
        .subscribe(res => {
            console.log(res);
            this.router.navigate(['device-info'], {queryParams: {deviceKey: this.newDevice}});
          },
          err => {
            console.log(err);
            if (err.status == 401) {
              this.errorMessage = 'You have not been invited to add this device to your list. Ask the owner.';
            } else {
              this.errorMessage = 'A device with the key ' + this.newDevice + ' does not exist';
            }
          });
    }
  }

  openInviteModal(deviceKey: string) {
    this.showInviteModal = true;
    this.inviteDeviceKey = deviceKey;
  }

  sendInvite() {
    this.inviteService.sendInvite(this.inviteDeviceKey, this.invite)
      .subscribe(res => {
        this.inviteSuccessMessage = 'An invite has been sent to ' + this.invite;
        setTimeout(() => {
          this.closeModal();
        }, 2000);
      }, err => {
        this.inviteErrorMessage = 'There is no user registered using this email address.'
      })
  }
}
