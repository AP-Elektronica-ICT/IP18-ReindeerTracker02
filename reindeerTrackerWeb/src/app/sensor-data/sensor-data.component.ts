import { Component, OnInit } from '@angular/core';
import { SensorDataService } from './sensor-data.service';
import { HttpClient } from 'selenium-webdriver/http';
import { element } from 'protractor';
import {AuthService} from "../shared/auth.service";

@Component({
  selector: 'app-sensor-data',
  templateUrl: './sensor-data.component.html',
  styleUrls: ['./sensor-data.component.css'],
  providers: [SensorDataService]
})
export class SensorDataComponent implements OnInit {
naam : string;
  constructor(private server: SensorDataService) { }

  ngOnInit() {
    //this.
  }
}
