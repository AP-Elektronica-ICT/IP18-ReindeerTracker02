import { Injectable } from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {AppSettings} from "./AppSettings";

@Injectable()
export class InviteService {
  url = AppSettings.API_ENDPOINT;

  constructor(private http: HttpClient) { }

  sendInvite(deviceKey: string, email: string) {
    console.log(deviceKey + ':' + email, 'inviting');
    return this.http.put(this.url + '/devices/' + deviceKey + '/invite', {email: email});
  }
}
