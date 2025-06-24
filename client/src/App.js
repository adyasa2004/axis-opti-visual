import React, { useState } from 'react';
import axios from 'axios';
import Plot from 'react-plotly.js';
import './App.css';

function App() {
  const [file, setFile] = useState(null);
  const [cost, setCost] = useState(null);
  const [rectangle, setRectangle] = useState([]);
  const [buildings, setBuildings] = useState([]);
  const [rawOutput, setRawOutput] = useState('');
  const [error, setError] = useState('');

  const handleFileChange = (e) => {
    setFile(e.target.files[0]);
  };

  const handleUpload = async () => {
    if (!file) return;

    const formData = new FormData();
    formData.append('file', file);

    try {
      const res = await axios.post("https://axis-backend-c9m2.onrender.com/optimize", formData);
      setCost(res.data.cost);
      setRectangle(res.data.rectangle);
      setBuildings(res.data.buildings || []);
      setRawOutput(res.data.raw_output || '');
      setError('');
    } catch (err) {
      console.error(err);
      setError('Something went wrong. Please try again.');
    }
  };

  return (
    <div className="App">
      <h1>Axis Enclosure Optimizer</h1>
      <input type="file" accept=".txt" onChange={handleFileChange} />
      <button onClick={handleUpload}>Run Optimizer</button>

      {cost !== null && <p><strong>Optimized Cost:</strong> {cost}</p>}
      {error && <p style={{ color: 'red' }}>{error}</p>}

      {(rectangle.length > 0 || buildings.length > 0) && (
        <Plot
          data={[
            {
              x: [...rectangle.map(p => p[0][0]), rectangle[0][0][0]],
              y: [...rectangle.map(p => p[0][1]), rectangle[0][0][1]],
              type: 'scatter',
              mode: 'lines+markers',
              name: 'Enclosure',
              line: { shape: 'linear', color: 'blue' },
            },
            {
              x: buildings.filter(b => b.weight >= 0).map(b => b.x),
              y: buildings.filter(b => b.weight >= 0).map(b => b.y),
              type: 'scatter',
              mode: 'markers',
              name: 'Positive Weight',
              marker: { color: 'green', size: 4 }
            },
            {
              x: buildings.filter(b => b.weight < 0).map(b => b.x),
              y: buildings.filter(b => b.weight < 0).map(b => b.y),
              type: 'scatter',
              mode: 'markers',
              name: 'Negative Weight',
              marker: { color: 'red', size: 4 }
            }
          ]}
          layout={{
            title: 'Optimized Rectangle and Buildings',
            xaxis: { title: 'X Axis' },
            yaxis: { title: 'Y Axis' },
            width: 800,
            height: 600,
          }}
        />
      )}

      {rawOutput && (
        <div style={{ textAlign: 'left', marginTop: '2rem', fontFamily: 'monospace' }}>
          <h3>Raw Optimizer Output:</h3>
          <pre style={{ background: '#f9f9f9', padding: '1rem', maxHeight: '300px', overflowY: 'auto' }}>
            {rawOutput}
          </pre>
        </div>
      )}
    </div>
  );
}

export default App;

