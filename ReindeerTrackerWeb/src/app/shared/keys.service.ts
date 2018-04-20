import { Injectable } from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs/Observable";
import {AppSettings} from "./AppSettings";

@Injectable()
export class KeysService {
  url = AppSettings.API_ENDPOINT;

  constructor(private httpClient: HttpClient) { }

  checkKeyLength(key: string): boolean {
    return key.length === 5;
  }

  addValidKeys(keys: string[]): Observable<object> {
    return this.httpClient.post(this.url + '/devices', keys);
  }

  addKeyToUser(uid: string, deviceKey: string): Observable<object> {
    return this.httpClient.put(this.url + '/users/' + uid + '/devices', {deviceKey: deviceKey});
  }
}
