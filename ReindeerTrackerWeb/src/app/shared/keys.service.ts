import { Injectable } from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs/Observable";

@Injectable()
export class KeysService {
  url = "http://localhost:3000/api";

  constructor(private httpClient: HttpClient) { }

  checkKeyLength(key: string): boolean {
    return key.length === 6;
  }

  addValidKeys(keys: string[]): Observable<object> {
    return this.httpClient.post(this.url + '/devices', keys);
  }

  addKeyToUser(uid: string, deviceKey: string): Observable<object> {
    return this.httpClient.put(this.url + '/users/' + uid + '/devices', {deviceKey: deviceKey});
  }

  activateDevice(deviceKey: string): Observable<object> {
    return this.httpClient.put(this.url + '/devices/' + deviceKey + '/activate', {})
  }

}
