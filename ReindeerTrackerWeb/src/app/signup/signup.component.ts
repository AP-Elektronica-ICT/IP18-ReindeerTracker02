import { Component, OnInit } from '@angular/core';
import {FormGroup} from "@angular/forms";
import {AuthService} from "../shared/auth.service";

@Component({
  selector: 'app-signup',
  templateUrl: './signup.component.html',
  styleUrls: ['./signup.component.css']
})
export class SignupComponent implements OnInit {
  errorMessage = null;

  constructor(private auth: AuthService) { }

  ngOnInit() {
  }

  signup(form: FormGroup) {
    this.errorMessage = null;
    const email = form.controls.email.value;
    const password =  form.controls.password.value;
    const passwordrpt = form.controls.passwordrpt.value;
    if (password === passwordrpt && form.valid) {
      this.auth.signupWithEmailPassword(email, password)
        .then(result => {
          console.log(result);
        })
        .catch(err => {
          this.errorMessage = 'There is already an account with this email address';
        })
    } else if (password !== passwordrpt) {
      this.errorMessage = 'The passwords do not match.';
    }
  }

}
