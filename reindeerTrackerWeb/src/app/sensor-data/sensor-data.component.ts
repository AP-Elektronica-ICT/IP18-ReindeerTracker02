import { Component, OnInit } from '@angular/core';
import { SensorDataService } from './sensor-data.service';

@Component({
  selector: 'app-sensor-data',
  templateUrl: './sensor-data.component.html',
  styleUrls: ['./sensor-data.component.css'],
  providers: [SensorDataService]
})
export class SensorDataComponent implements OnInit {

  lat: number;
  long: number;

  constructor(private server: SensorDataService) { }

  ngOnInit() {
    this.server.getSensorData().subscribe(data => this.lat = data);
    this.server.getSensorData().subscribe(data => this.long = data);
  }

}
