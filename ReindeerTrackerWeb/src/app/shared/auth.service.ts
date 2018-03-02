import { Injectable } from '@angular/core';

@Injectable()
export class AuthService {

  constructor() { }

  getCurrentUID(): string {
    //TODO get uid from firebase login
    return 'A1B2C3';
  }

}
