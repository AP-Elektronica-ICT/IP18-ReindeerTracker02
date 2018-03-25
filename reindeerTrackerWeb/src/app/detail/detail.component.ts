import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-detail',
  templateUrl: './detail.component.html',
  styleUrls: ['./detail.component.css']
})
export class DetailComponent implements OnInit {
// AGM static -----
lat: number = 51.167711;
lng: number = 4.070101;
rad: number = 600;
col: string = 'green';
op: number = 0.3;

ft(){
  return "terrain";
}
// AGM static -----
  constructor() { }

  ngOnInit() {
  }

}
