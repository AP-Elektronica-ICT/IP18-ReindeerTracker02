import { Component, OnInit } from '@angular/core';
import {FormGroup} from "@angular/forms";
import {AuthService} from "../shared/auth.service";
import {Router} from "@angular/router";

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {
  showErrorMessage = false;

  constructor(private auth: AuthService, private router: Router) { }

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
          this.router.navigate(['/']);
        })
        .catch(err => {
          this.showErrorMessage = true;
        })
    }
  }

}
