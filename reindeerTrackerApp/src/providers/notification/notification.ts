import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {AuthProvider} from "../auth/auth";
import {AppSettings} from "../../AppSettings";

/*
  Generated class for the NotificationProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class NotificationProvider {
  api = AppSettings.API_ENDPOINT;

  constructor(public http: HttpClient, private auth: AuthProvider) {

  }

  getNotifications() {
    const uid = this.auth.getCurrentUID();
    return this.http.get(this.api + '/users/' + uid + '/notifications');
  }

  deleteNotification(notificationID: string) {
    const uid = this.auth.getCurrentUID();
    return this.http.delete(this.api + '/users/' + uid + '/notifications/' + notificationID);
  }

  markNotificationsAsSeen(notificationIDs: string[]) {
    const uid = this.auth.getCurrentUID();
    return this.http.put(this.api + '/users/' + uid + '/notifications/seen', notificationIDs);
  }

}
