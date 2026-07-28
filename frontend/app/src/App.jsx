import './App.css'
import ChessBoardComponent from './ChessBoard'
import { Analytics } from '@vercel/analytics/react'

function App() {
  return (
    <div className="app-container">
      <ChessBoardComponent />
      <Analytics />
    </div>
  )
}

export default App
