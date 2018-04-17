import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {LoadingController} from "ionic-angular";

/*
  Generated class for the LoadingProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class LoadingProvider {
  loginLoading = null;
  deviceLoading = null;

  constructor(public loadingCtrl: LoadingController) {
    this.loginLoading = this.loadingCtrl.create({
      content: 'Logging in...'
    });
    this.deviceLoading = this.loadingCtrl.create({
      content: 'Getting devices...'
    });
  }

  presentLoginLoading() {
    this.loginLoading.present();
  }

  dismissLoginLoading() {
    this.loginLoading.dismiss();
  }

  presentDeviceLoading() {
    this.deviceLoading.present();
  }

  dismissDeviceLoading() {
    this.deviceLoading.dismiss();
  }

}
