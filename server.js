/*const { createServer } = require('node:http');
const hostname = '127.0.0.1';
const port = 3000;

const server = createServer((req, res) => {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/plain');
  res.end('Hello World');
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});*/
const cors = require('cors');
const express = require('express');
const { execFile } = require('child_process');
const app = express();

const go_to_location_script_path = './go_to_location/build/go_to_location';
const deliver_payload_script_path = './deliver_payload/build/deliver_payload';

app.use(cors({origin: 'http://127.0.0.1:5500'}));
app.use(express.json());

// Post handling endpoint
app.post('/api/go_to_location', (req, res) => {
  const latitude = req.body.latitude.toString();
  const longitude = req.body.longitude.toString();
  console.log(`starting go_to_location script with coordinates Lat: ${latitude} , Lng: ${longitude}`);

  execFile(go_to_location_script_path, [latitude, longitude], (error, stdout, stderr) => {
     if (error) {
        console.error(`Execution Error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.error(`Standard Error: ${stderr}`);
        return;
    }
    console.log(`Script Output:\n${stdout}`);
  })


  res.json({
        status: "success",
        name: "Duncan Idaho",
        role: "pad footed seeli fit only for slinging slig shit"
    });
})

// Get handling endpoint
app.get('/api/go_to_location', (req, res) => {
  // All logic in get(..) callback func runs when request made to path
  execFile(deliver_payload_script_path, ['34.6767', '12.0139'], (error, stdout, stderr) => {
     if (error) {
        console.error(`Execution Error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.error(`Standard Error: ${stderr}`);
        return;
    }
    console.log(`C++ Output:\n${stdout}`);
  })
  res.json({
        id: 42,
        name: "Tony Stark",
        role: "Genius, philanthropist, billionaire, playboy, not a painter"
    });
});

// Post handling endpoint
app.post('/api/deliver_payload', (req, res) => {
  const latitude = req.body.latitude.toString();
  const longitude = req.body.longitude.toString();
  console.log(`starting deliver_payload script with coordinates Lat: ${latitude} , Lng: ${longitude}`);

  execFile(deliver_payload_script_path, [latitude, longitude], (error, stdout, stderr) => {
     if (error) {
        console.error(`Execution Error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.error(`Standard Error: ${stderr}`);
        return;
    }
    console.log(`Script Output:\n${stdout}`);
  })

// Testing Cloudflare; disregard me if not removed
  res.json({
        status: "greeeet",
        name: "Miles Teg",
        role: "Putting up with none of the frumpets"
    });
})

// Get handling endpoint
app.get('/api/deliver_payload', (req, res) => {
  // All logic in get(..) callback func runs when request made to path
  execFile(go_to_location_script_path, ['34.6767', '12.0139'], (error, stdout, stderr) => {
     if (error) {
        console.error(`Execution Error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.error(`Standard Error: ${stderr}`);
        return;
    }
    console.log(`C++ Output:\n${stdout}`);
  })
  res.json({
        id: 22,
        name: "Darwi Odrade",
        role: "Mother Superior"
    });
});

app.listen(3000, () => console.log('Server running on port 3000'));

