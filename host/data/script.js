
var map = L.map('map'),
trail = {
type: 'Feature',
properties: {
id: [],
color: [],
name: [],
popupContent: []
 },
geometry: {
type: 'LineString',
coordinates: []
},
},

ws = new WebSocket("ws://192.168.1.181/ws");
realtime = L.realtime(function(success, error) {
(ws.onmessage = function(data) {
 var jsonData = JSON.parse(data.data);
 console.log(jsonData)
 var trailCoords = trail.geometry.coordinates;
            trailCoords.push(jsonData.geometry.coordinates);
            trailCoords.splice(0, Math.max(0, trailCoords.length - 5));
var trailProps = trail.properties.id;
            trailProps.push(jsonData.properties.id);
success({
type: 'Feature',
features: [data, trail],
properties: [data, trail]
});
});

}, {
interval: 3 * 1000
}).addTo(map);

L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {
attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
}).addTo(map);

L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {
attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
}).addTo(map);

realtime.on('update', function() {
    map.fitBounds(realtime.getBounds(), {maxZoom: 19});
});
