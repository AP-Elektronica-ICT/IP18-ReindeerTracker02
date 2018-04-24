import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';

/*
  Generated class for the DateformatProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class DateformatProvider {

  constructor() {

  }

  formatDate(date: Date): string {
    return date.getDay() + '/' + (date.getMonth() + 1) + '/' + date.getFullYear();
  }

}
