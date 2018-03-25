import { Injectable } from '@angular/core';
import { Http, Response } from "@angular/http";
import { Observable } from "rxjs/Observable";
import "rxjs/add/operator/map";
import { HttpClient } from '@angular/common/http';

@Injectable()
export class SensorDataService {

  private _ip: string = "http://localhost:3000/api/users/fff/devices";

  constructor(private _http: HttpClient) { }

  getSensorData() {
    // return this._http.get(this._ip).map(response => response.json());
}

}
