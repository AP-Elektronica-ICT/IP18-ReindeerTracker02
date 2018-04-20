import { Injectable } from '@angular/core';
import {Platform, ToastController} from "ionic-angular";
import {Firebase} from "@ionic-native/firebase";
import {AppSettings} from "../../AppSettings";
import {HttpClient} from "@angular/common/http";
import {AuthProvider} from "../auth/auth";

/*
  Generated class for the FcmProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class FcmProvider {
  api = AppSettings.API_ENDPOINT;

  constructor(public firebase: Firebase, private platform: Platform, private httpClient: HttpClient, private auth: AuthProvider) {

  }

  async getToken() {
    let token;
    if (this.platform.is('android')) {
      token = await this.firebase.getToken();
    }
    if (this.platform.is('ios')) {
      token = await this.firebase.getToken();
      await  this.firebase.grantPermission();
    }

    this.saveToken(token);
  }

  setFirebaseOnRefreshToken() {
    this.firebase.onTokenRefresh().subscribe(token => {
      this.saveToken(token);
    });
  }

  getTokenSync() {
    let token = this.firebase.getToken();
    this.saveToken(token);
  }

  private saveToken(token) {
    const uid = this.auth.getCurrentUID();
    this.httpClient.put(this.api + '/users/' + uid + '/devicetoken', {deviceToken: token})
      .subscribe(res => {
        console.log(res);
      })
  }

  public removeToken() {
    const uid = this.auth.getCurrentUID();
    this.httpClient.delete(this.api + '/users/' + uid + '/devicetoken')
      .subscribe(res => {
        console.log(res);
      })
  }

  listenToNotifications() {
    return this.firebase.onNotificationOpen();
  }

}
