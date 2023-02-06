import React, {useRef} from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import { clone, cloneDeep } from "lodash"
import reportWebVitals from './reportWebVitals';

function Square(props) {
  return (
    <div className="square" onClick={props.onClick}  style={{backgroundColor : props.highlight ? 'green' : 'white'}}>
      {props.value}
    </div>
    
  );
}
class Piece extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      highlight : false,
      team : 0,
      firstMove: true,
    };

  }
  render() {
    //console.log(this.props);
    return (
      <Square
        value={this.props.value}
        highlight = {this.props.highlight}
        onClick = {()=>this.props.onClick()}

      />
    )
  }

}

class Board extends React.Component {

  renderSquare(i,j) {
    const square = this.props.squares[i][j];
    return (
      <Piece
        value={this.props.squares[i][j].value}
        onClick={() => this.props.onClick(i,j)}
        key={(i.toString()+j.toString()).toString()}
        highlight={this.props.squares[i][j].highlight}

      />
    );
  }


  render() {
    let content = [];
    for(let i = 0;i<8;i++)
    {
      content.push(<div className="board-row" key={i.toString()}></div>)
      for (let j=0;j<8;j++)
      {
        content.push(this.renderSquare(i,j));
      }

    }
    return (
      <div>
        {content}
      </div>
    );
  }
}
class Game extends React.Component {
  constructor(props) {
    super(props);
    const row  = []
    for(let i=0;i<8;i++)
    {
      row.push(new Piece())
    }
    const squares = Array(8).fill(0).map(() => cloneDeep(row))
    initializeBoard(squares)
    this.state = {
      history: [
        {
        squares: squares
        }
      ],
      moveNumber: 0,
      play1Next: true,
      previousMoves: [],
      grabbedBlock: [],
    };

  }


  handleClick(i,j) {
    
    const history = this.state.history.slice(0,this.state.moveNumber+1);
    const current = history[history.length-1];
    const squares = current.squares.slice();
    console.log(this.state);
    if ((this.state.play1Next && squares[i][j].state.team == 2 )|| (!this.state.play1Next && squares[i][j].state.team == 1))
    {
      return;
    }
    //squares[i][j].highlight = true;
    
    if(this.state.grabbedBlock.length == 0)
    {
      if(squares[i][j].value != ' ' && squares[i][j].value)
      {
        const moves = highlightMoves(history[history.length-1].squares,i,j);
        if(moves.length > 0) {
          this.setState({grabbedBlock: [i,j]})
          this.state.grabbedBlock = [i,j];
        }
      }
      
      return;
    }
    else{
      if(squares[i][j].highlight != true)
      {
        for(let y=0;y<squares.length;y++) {
          for(let z = 0;z<squares[y].length;z++) {squares[y][z].highlight = false;}
        }
        this.setState({grabbedBlock : []});
      }
      else{
        //squares[i][j]
        highlightMoves(squares,this.state.grabbedBlock[0],this.state.grabbedBlock[1]);
        squares[i][j].value = squares[this.state.grabbedBlock[0]][this.state.grabbedBlock[1]].value;
        squares[this.state.grabbedBlock[0]][this.state.grabbedBlock[1]].value = null;
        this.setState({grabbedBlock: []})
        this.setState({play1Next: !this.state.play1Next});
        this.setState({firstMove: false})
      }

      
    }

    /*     this.setState({
      history: history.concat([
        {
          squares : squares
        }

      ]),
      stepNumber: history.length,
      play1Next : !this.state.play1Next,

    }); */
  }

  handleDrag(i,j) {
    console.log(i,j);
    const history = this.state.history.slice(0,this.state.moveNumber+1);
    const moves = highlightMoves(history[history.length-1].squares,i,j)

    //this.setState({previousMoves: moves})
    //this.setState({stepNumber: history.length});
  }

  handleDrop(e,i,j) {
    console.log("A");
    //e.preventDefault();
    
    return;
    if(!this.state.history[this.state.history.length-1].squares[i][j].highlight)
    {
      return;
    }
    else {
      for(let i = 0;i<this.previousMoves;i++)
      {
        this.state.history[this.state.highlight.length-1].squares[this.previousMoves[i][0]].highlight = false;
        this.state.history[this.state.highlight.length-1].squares[this.previousMoves[i][1]].highlight = false;
        this.setState({previousMoves:[]});
      }
    }
  }


  render() {
    const history = this.state.history;
    const current = history[this.state.moveNumber];

    return (
      <div className="game">
        <div className="game-board">
          <Board
            squares={current.squares}
            onClick={(i,j) => this.handleClick(i,j)}
          />
        </div>
        <div className="game-info">

        </div>
      </div>
    );
  }
}

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(<Game />);

function calculateWinner(squares)
{
  //TODO
  return false;
}

function initializeBoard(squares)
{
    const copy1 = squares[1].slice();
    const copy2 = squares[6].slice();
    for(let j=0;j<8;j++)
    {
      squares[1][j].value = 'P';
      squares[1][j].state.team = 2;
      squares[6][j].value = 'P';
      squares[6][j].state.team = 1;
    }

    squares[0][0].value='R';squares[0][1].value='N';squares[0][2].value='B';squares[0][3].value='Q';
    squares[0][4].value='K';squares[0][5].value='B';squares[0][6].value='N';squares[0][7].value='R';
    squares[7][0].value='R';squares[7][1].value='N';squares[7][2].value='B';squares[7][3].value='Q';
    squares[7][4].value='K';squares[7][5].value='B';squares[7][6].value='N';squares[7][7].value='R';
    for(let i=0;i<8;i++) {squares[0][i].state.team = 2;squares[7][i].state.team = 1;}
}

function highlightMoves(squares,i,j)
{ 
  console.log(squares);
  const moves = []
  const options = new Map();
  options.set('N',[[2,-1],[1,-2],[2,1],[1,2],[-1,-2],[-2,-1],[-1,2],[-2,1]]);
  options.set('K',[[1,0],[-1,0],[1,1],[-1,-1],[0,1],[0,-1],[-1,1],[1,-1]]);
  // Special cases must use while loop
  options.set('B',[[1,1],[1,-1],[-1,-1],[-1,1]]);
  options.set('Q',[[1,0],[-1,0],[1,1],[-1,-1],[0,1],[0,-1],[-1,1],[1,-1]]);
  options.set('R',[[1,0],[-1,0],[0,1],[0,-1]]);
  const pieceMoves = options.get(squares[i][j].value);
  //console.log(squares);
  if (squares[i][j].value == 'P')
  {
    let k = 1;
    if (squares[i][j].state.team == 2) {k = -1;}
    if ((i-k) >= 0 && !squares[i-k][j].value)
    {
      moves.push([i-k,j])
    }
    if (squares[i][j].state.firstMove && !squares[i-(k*2)][j].value && !squares[i-k][j].value)
    {
      moves.push([i-(k*2),j])
    }
  }
  else{
    for (let k=0;k<pieceMoves.length;k++)
    {
      const vert = pieceMoves[k][0];
      const lat = pieceMoves[k][1];
      
      if((i+vert) >= 0 && (i+vert)<=7 && (j+lat) >= 0 && (j+lat) <= 7)
      {
        if(!squares[i+vert][j+lat].value || squares[i+vert][j+lat].state.team != squares[i][j].state.team)
        {
          moves.push([i+vert,j+lat])
        }
        
      }
    }
  }
  for (let i=0;i<moves.length;i++)
  {
    squares[moves[i][0]][moves[i][1]].highlight = !squares[moves[i][0]][moves[i][1]].highlight;
  }
  return moves;
}
// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
