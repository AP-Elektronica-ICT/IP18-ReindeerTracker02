import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {AliveState, BatteryState, FilterOptions} from "../../classes/filterOptions";

/*
  Generated class for the FilterProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class FilterProvider {
  filterOptions: FilterOptions;

  constructor() {
    this.filterOptions = {
      alive: AliveState.all,
      battery: BatteryState.all
    }
  }

  getFilterOptions(): FilterOptions {
    return JSON.parse(JSON.stringify(this.filterOptions));
  }

  setFilterOptions(filterOptions: FilterOptions) {
    this.filterOptions = filterOptions;
  }

}
