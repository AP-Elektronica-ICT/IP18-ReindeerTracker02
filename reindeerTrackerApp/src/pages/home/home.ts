import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { DetailPage } from '../detail/detail';
import { DeletePage } from '../delete/delete';
import { AddPage } from '../add/add';
import { ProfilePage } from '../profile/profile';
import { LogInPage } from '../log-in/log-in';
import {DeviceProvider} from "../../providers/device/device";

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {
  devices: any = null;

  constructor(public navCtrl: NavController, private deviceProvider: DeviceProvider) {
    this.getDevices();
  }

  getDevices() {
    this.deviceProvider.getUserDevices()
      .subscribe(res => {
        this.devices = res;
        console.log(this.devices);
      })
  }

  loadDetail(deviceKey: string){
    this.navCtrl.push(DetailPage, deviceKey);
  }

  loadDelete(){
    this.navCtrl.push(DeletePage);
  }

  loadAdd(){
    this.navCtrl.push(AddPage);
  }

  loadProfile(){
    this.navCtrl.push(ProfilePage);
  }

  loadLogOut(){
    this.navCtrl.push(LogInPage);
  }

}
