import { Component, OnInit } from '@angular/core';
import {FormGroup} from "@angular/forms";
import {AuthService} from "../shared/auth.service";

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {
  showErrorMessage = false;

  constructor(private auth: AuthService) { }

  ngOnInit() {
  }

  login(form: FormGroup) {
    this.showErrorMessage = false;
    const email = form.controls.email.value;
    const password = form.controls.password.value;
    if (form.valid) {
      this.auth.loginWithEmailPassword(email, password)
        .then(res => {
          console.log(res);
        })
        .catch(err => {
          this.showErrorMessage = true;
        })
    }
  }

}
