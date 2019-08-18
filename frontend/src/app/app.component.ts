import { Component } from '@angular/core';
import {RestService} from './rest.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'projetoFinalSO';

  constructor(private http: RestService) {}

  goAhead() {
    this.http.goAhead().subscribe();
  }

  turnLeft() {
    this.http.turnLeft().subscribe();
  }

  turnRight() {
    this.http.turnRight().subscribe();
  }

  reset() {
    this.http.reset().subscribe();
  }
}
