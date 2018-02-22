import { Injectable } from '@angular/core';
import {HttpClient} from "@angular/common/http";

@Injectable()
export class KeysService {
  url = "http://localhost:3000/api";

  constructor(private httpClient: HttpClient) { }

  addValidKeys(keys: string[]) {
    return this.httpClient.post(this.url + '/devices', keys);
  }

}
