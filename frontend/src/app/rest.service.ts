import {Injectable} from '@angular/core';
import {HttpClient, HttpHeaders} from '@angular/common/http';
import {Observable} from 'rxjs';
import {map} from 'rxjs/operators';

const endpoint = 'http://localhost:5000/';
const httpOptions = {
  headers: new HttpHeaders({
    'Content-Type':  'application/json'
  })
};

@Injectable({
  providedIn: 'root'
})
export class RestService {
  constructor(private http: HttpClient) { }

  private extractData(res: Response) {
    return res || { };
  }

  goAhead(): Observable<any> {
    return this.http.post(endpoint + 'set/1', ' ').pipe(
      map(this.extractData));
  }

  turnLeft(): Observable<any> {
    return this.http.post(endpoint + 'set/2', ' ').pipe(
      map(this.extractData));
  }

  turnRight(): Observable<any> {
    return this.http.post(endpoint + 'set/3', ' ').pipe(
      map(this.extractData));
  }

 reset(): Observable<any> {
    return this.http.delete(endpoint + 'reset').pipe(
      map(this.extractData));
  }
}
