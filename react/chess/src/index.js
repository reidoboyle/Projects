import React, {useRef} from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import { clone, cloneDeep } from "lodash"
import reportWebVitals from './reportWebVitals';
const UNICODE_PIECES =  [
  ['r','♜'], ['n','♞'], ['b','♝'], ['q','♛'],
  ['k','♚'], ['p','♟'], ['R','♖'], ['N','♘'],
  ['B','♗'], ['Q','♕'], ['K','♔'], ['P','♙'],
  [null,' ']
]
const PIECE_MAP = new Map();
for(let i=0;i<UNICODE_PIECES.length;i++) {PIECE_MAP.set(UNICODE_PIECES[i][0],UNICODE_PIECES[i][1])};
function Square(props) {
  return (
    <div className="square" onClick={props.onClick}  style={{backgroundColor : props.checked ? 'red' : props.highlight ? 'green' : 'beige' }}>
      {PIECE_MAP.get(props.value)}
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
      checked: false,
    };

  }


  render() {
    console.log(this.state.highlight);
    return (
      <Square
        value={this.props.value}
        highlight = {this.props.highlight}
        onClick = {()=>this.props.onClick()}
        checked = {this.props.checked}

      />
    )
  }

}

class Board extends React.Component {

  renderSquare(i,j) {
    //const square = this.props.squares[i][j];
    let val = this.props.squares[i][j].value;
    if (this.props.squares[i][j].state.team == 2 && this.props.squares[i][j].value) {val = val.toLowerCase();}
    return (
      <Piece
        value={val}
        onClick={() => this.props.onClick(i,j)}
        key={(i.toString()+j.toString()).toString()}
        highlight={this.props.squares[i][j].highlight}
        checked={this.props.squares[i][j].checked}
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
    initializeBoard(squares);
    this.state = {
      //history: [
        //{
      squares: squares,
        //}
      //],
      moveNumber: 0,
      play1Next: true,
      previousMoves: [],
      grabbedBlock: [],
      checked: false,
      gameOver: false,
    };

  }


  handleClick(i,j) {
    
    //const history = this.state.history.slice(0,this.state.moveNumber+1);
    //const current = history[history.length-1];
    const squares = this.state.squares;
    if (this.state.grabbedBlock.length == 0 && ((this.state.play1Next && squares[i][j].state.team == 2 )|| (!this.state.play1Next && squares[i][j].state.team == 1)))
    {
      return;
    }
    if(this.state.checked)
    {
      const moves = this.highlightMoves(this.state.squares,i,j);
      for(let z=0;z<moves.length;z++)
      {
      }
    }
    //squares[i][j].highlight = true;
    
    if(this.state.grabbedBlock.length == 0)
    {
      if(squares[i][j].value != ' ' && squares[i][j].value)
      {
        const moves = this.highlightMoves(this.state.squares,i,j);
        console.log(this.state.squares);

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
        this.highlightMoves(squares,this.state.grabbedBlock[0],this.state.grabbedBlock[1]);
        squares[i][j].value = squares[this.state.grabbedBlock[0]][this.state.grabbedBlock[1]].value;
        squares[this.state.grabbedBlock[0]][this.state.grabbedBlock[1]].value = null;
        this.setState({grabbedBlock: []})
        this.setState({play1Next: !this.state.play1Next});
        squares[i][j].state.team = squares[this.state.grabbedBlock[0]][this.state.grabbedBlock[1]].state.team;
        squares[i][j].state.firstMove = false;
      }
      const king = this.isChecked(squares,squares[i][j].state.team)
      if(king.length > 0) {
        squares[king[0]][king[1]].checked = true;
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
  highlightMoves(squares,i,j,dontHighlight=false)
  { 
    //console.log(squares)

    const moves = []
    const options = new Map();
    squares = this.state.squares;
    options.set('N',[[2,-1],[1,-2],[2,1],[1,2],[-1,-2],[-2,-1],[-1,2],[-2,1]]);
    options.set('K',[[1,0],[-1,0],[1,1],[-1,-1],[0,1],[0,-1],[-1,1],[1,-1]]);
    // Special cases must use while loop
    options.set('B',[[1,1],[1,-1],[-1,-1],[-1,1]]);
    options.set('Q',[[1,0],[-1,0],[1,1],[-1,-1],[0,1],[0,-1],[-1,1],[1,-1]]);
    options.set('R',[[1,0],[-1,0],[0,1],[0,-1]]);
    let pieceMoves = options.get(squares[i][j].value);
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
      if ( j+1 <= 7 &&(squares[i-k][j+1].state.team != squares[i][j].state.team) && squares[i-k][j+1].state.team != 0)
      {
        moves.push([i-k,j+1])
      }
      if (j-1 >= 0 && (squares[i-k][j-1].state.team != squares[i][j].state.team) && squares[i-k][j-1].state.team != 0)
      {
        moves.push([i-k,j-1])
      }
    }
    else{
      // special cases
      if(squares[i][j].value == 'R' || squares[i][j].value == 'B' || squares[i][j].value == 'Q') {
        while(pieceMoves.length > 0)
        {
          let newMoves = [];
          for (let k=0;k<pieceMoves.length;k++) {
            let vert = pieceMoves[k][0];
            let lat = pieceMoves[k][1];
            if((i+vert) >= 0 && (i+vert)<=7 && (j+lat) >= 0 && (j+lat) <= 7)
            {
              if(!squares[i+vert][j+lat].value || squares[i+vert][j+lat] == ' ')
              {
                //console.log(vert+vert/vert,lat+lat/getSpaceUntilMaxLength());
                moves.push([i+vert,j+lat]);
                if(vert <0) {vert -= 1;} else if(vert > 0) {vert += 1}
                if(lat <0) {lat -= 1;} else if (lat > 0) {lat += 1}
                newMoves.push([vert,lat]);
              }
              else if(squares[i+vert][j+lat].state.team != squares[i][j].state.team)
              {
                moves.push([i+vert,j+lat]);
              }
            }
          }
          pieceMoves = newMoves;
        }
      }
      else {
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
      
      
    }
    //console.log(moves);
    if(!dontHighlight)
    {
      for (let i=0;i<moves.length;i++)
      {
        
        //this.state.history[this.state.history.length-1].squares[moves[i][0]][moves[i][1]].state.highlight = true;//!this.squares[moves[i][0]][moves[i][1]].state.highlight;
        this.state.squares[moves[i][0]][moves[i][1]].highlight = !this.state.squares[moves[i][0]][moves[i][1]].highlight
      }
    }
    return moves;
  }

  isChecked(squares,teamNum)
  {
    // find the king
    let kingI = 0;
    let kingJ = 0;
    for(let y=0;y<squares.length;y++)
    {
      for(let z=0;z<squares[y].length;z++)
      {
        
        if(squares[y][z].value == 'K' && squares[y][z].state.team != teamNum)
        {
          kingI = y;
          kingJ = z;
        }
      }
    }
  
    let moves = [];
    for(let y=0;y<squares.length;y++)
    {
      for(let z=0;z<squares[y].length;z++)
      {
        if(squares[y][z].value && squares[y][z].state.team == teamNum)
        {
          moves = this.highlightMoves(squares,y,z,true);
          for(let w=0;w<moves.length;w++)
          {
            if (moves[w][0] == kingI && moves[w][1] == kingJ) {
              return [kingI,kingJ];
            }
          }
        }
      }
    }
  
    return [];
  }

  render() {
    //const history = this.state.history;
    //const current = history[this.state.moveNumber];
    return (
      <div className="game">
        <div className="game-board">
        <p className='title'>{this.state.play1Next ? 'White' : 'Black'}'s move </p>
        {this.state.gameOver ? <p>Game Over</p> : ''}
          <Board
            squares={this.state.squares}
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
    for(let i=0;i<8;i++)
    {
      for(let j=0;j<8;j++)
      {
        squares[i][j].state.highlight = false;
      }
    }

    squares[0][0].value='R';squares[0][1].value='N';squares[0][2].value='B';squares[0][3].value='Q';
    squares[0][4].value='K';squares[0][5].value='B';squares[0][6].value='N';squares[0][7].value='R';
    squares[7][0].value='R';squares[7][1].value='N';squares[7][2].value='B';squares[7][3].value='Q';
    squares[7][4].value='K';squares[7][5].value='B';squares[7][6].value='N';squares[7][7].value='R';
    for(let i=0;i<8;i++) {squares[0][i].state.team = 2;squares[7][i].state.team = 1;}
}




// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
