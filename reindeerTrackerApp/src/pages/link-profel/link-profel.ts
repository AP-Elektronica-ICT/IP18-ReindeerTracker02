import { Component } from '@angular/core';
import {IonicPage, NavController, NavParams, ToastController} from 'ionic-angular';
import {HttpClient} from "@angular/common/http";
import { DeviceProvider } from '../../providers/device/device';
import { Device } from '../../classes/device';

/**
 * Generated class for the LinkProfelPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-link-profel',
  templateUrl: 'link-profel.html',
})
export class LinkProfelPage {

  device: Device = null;
  deviceKey = '';
  emailAdress = '';

  constructor(public navCtrl: NavController, public navParams: NavParams, private http: HttpClient, private deviceProvider: DeviceProvider, private toastCtrl: ToastController) {
  }

  ionViewDidLoad() {
    this.deviceKey = this.navParams.get('deviceKey');
    console.log('ionViewDidLoad LinkProfelPage');

    this.deviceProvider.getDevice(this.deviceKey)
      .subscribe((details: Device) => {
        this.device = details;
      })
  }

  sendInvite() {
    console.log(this.deviceKey + ':' + this.emailAdress, 'inviting');
    this.deviceProvider.putInvite(this.deviceKey, this.emailAdress)
      .subscribe(res => {
        console.log(res);
        this.openToast();
        this.navCtrl.pop();
      })
  }

  openToast() {
    let toast = this.toastCtrl.create({
      message: 'Ivitation sent to ' + this.emailAdress,
      duration: 3000
    });
    toast.present();
  }

}
