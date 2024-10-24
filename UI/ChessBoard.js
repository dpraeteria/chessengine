import React, { useState } from "react";
import { useEffect } from 'react';
import "./ChessBoard.css";

const ChessBoard = () => {
  const columns = ['none','a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']; // 가로축

  // 백 폰의 초기 위치
  const initialPawnPositions = [
    { row: 2, col: 1 }, // a2
    { row: 2, col: 2 }, // b2
    { row: 2, col: 3 }, // c2
    { row: 2, col: 4 }, // d2
    { row: 2, col: 5 }, // e2
    { row: 2, col: 6 }, // f2
    { row: 2, col: 7 }, // g2
    { row: 2, col: 8 }, // h2
  ];

  // 흑 폰의 초기 위치
  const initialBlackPawnPositions = [
    { row: 7, col: 1 }, // a7
    { row: 7, col: 2 }, // b7
    { row: 7, col: 3 }, // c7
    { row: 7, col: 4 }, // d7
    { row: 7, col: 5 }, // e7
    { row: 7, col: 6 }, // f7
    { row: 7, col: 7 }, // g7
    { row: 7, col: 8 }, // h7
  ];

  const initialRookPositions = [
    { row: 1, col: 1 }, // a1 (백 룩)
    { row: 1, col: 8 }, // h1 (백 룩)
  ];

  const initialBlackRookPositions = [
    { row: 8, col: 1 }, // a8 (흑 룩)
    { row: 8, col: 8 }, // h8 (흑 룩)
  ];

  const initialKnightPositions = [
    { row: 1, col: 2 }, // b1 (백 나이트)
    { row: 1, col: 7 }, // g1 (백 나이트)
  ];

  const initialBlackKnightPositions = [
    { row: 8, col: 2 }, // b8 (흑 나이트)
    { row: 8, col: 7 }, // g8 (흑 나이트)
  ];

  const initialBishopPositions = [
    { row: 1, col: 3 }, // c1 (백 비숍)
    { row: 1, col: 6 }, // f1 (백 비숍)
  ];

  const initialBlackBishopPositions = [
    { row: 8, col: 3 }, // c8 (흑 비숍)
    { row: 8, col: 6 }, // f8 (흑 비숍)
  ];


  const initialQueenPositions = [{ row: 1, col: 4 }]; // d1 (백 퀸)
  const initialBlackQueenPositions = [{ row: 8, col: 4 }]; // d8 (흑 퀸)

  const initialKingPositions = [{ row: 1, col: 5 }]; // e1 (백 킹)
  const initialBlackKingPositions = [{ row: 8, col: 5 }]; // e8 (흑 킹)

  const [selectedPiece, setSelectedPiece] = useState(null); // 선택된 기물
  const [selectedSquare, setSelectedSquare] = useState({ row: null, col: null }); // 선택된 칸
  const [selectedType, setSelectedType] = useState(null); // 선택된 기물의 타입
  const [isBlackTurn, setIsBlackTurn] = useState(false); // 턴 관리

  const [pawnsPosition, setPawnsPosition] = useState(initialPawnPositions); // 백 폰 위치
  const [blackPawnsPosition, setBlackPawnsPosition] = useState(initialBlackPawnPositions); // 흑 폰 위치
  const [rooksPosition, setRooksPosition] = useState(initialRookPositions); // 백 룩 위치
  const [blackRooksPosition, setBlackRooksPosition] = useState(initialBlackRookPositions); // 흑 룩 위치
  const [knightsPosition, setKnightsPosition] = useState(initialKnightPositions); // 백 나이트 위치
  const [blackKnightsPosition, setBlackKnightsPosition] = useState(initialBlackKnightPositions); // 흑 나이트 위치
  const [bishopsPosition, setBishopsPosition] = useState(initialBishopPositions); // 백 비숍 위치
  const [blackBishopsPosition, setBlackBishopsPosition] = useState(initialBlackBishopPositions); // 흑 비숍 위치
  const [queenPosition, setQueenPosition] = useState(initialQueenPositions); // 백 퀸 위치
  const [blackQueenPosition, setBlackQueenPosition] = useState(initialBlackQueenPositions); // 흑 퀸 위치
  const [kingPosition, setKingPosition] = useState(initialKingPositions); // 백 킹 위치
  const [blackKingPosition, setBlackKingPosition] = useState(initialBlackKingPositions); // 흑 킹 위치



    const handleSquareClick = (row, col) => {
      const columnLetter = columns[col]; // a~h 중 하나
      const rowNumber = 8 - row;         // 1~8 중 하나
      console.log(`${columnLetter}${rowNumber}`); // 클릭한 좌표를 콘솔에 출력
      
      setSelectedSquare({ row, col }); // 클릭된 칸을 선택 상태로 저장
    
      // 기물 이동 처리
      const movePiece = (positions, setPosition, pieceIndex) => {
        const updatedPositions = [...positions];
        updatedPositions[pieceIndex] = { row, col };
        setPosition(updatedPositions);
        setSelectedPiece(null);
        setSelectedType(null);
        setIsBlackTurn(!isBlackTurn); // 턴을 전환
      };
    
      // 흑 말을 잡는 로직 추가
      const capturePiece = (positions, setPosition) => {
        const pieceIndex = positions.findIndex((piece) => piece.row === row && piece.col === col);
        if (pieceIndex !== -1) {
          const updatedPositions = [...positions];
          updatedPositions.splice(pieceIndex, 1); // 해당 말을 제거
          setPosition(updatedPositions);
          return true; // 말을 잡았음을 반환
        }
        return false; // 잡지 않았음을 반환
      };
    
      if (selectedPiece !== null && selectedType) {
        // 백 차례일 때 흑 말을 잡는 로직
        const pieceCaptured = !isBlackTurn
          ? capturePiece(blackPawnsPosition, setBlackPawnsPosition) ||
            capturePiece(blackRooksPosition, setBlackRooksPosition) ||
            capturePiece(blackKnightsPosition, setBlackKnightsPosition) ||
            capturePiece(blackBishopsPosition, setBlackBishopsPosition) ||
            capturePiece(blackQueenPosition, setBlackQueenPosition) ||
            capturePiece(blackKingPosition, setBlackKingPosition)
          : capturePiece(pawnsPosition, setPawnsPosition) ||
            capturePiece(rooksPosition, setRooksPosition) ||
            capturePiece(knightsPosition, setKnightsPosition) ||
            capturePiece(bishopsPosition, setBishopsPosition) ||
            capturePiece(queenPosition, setQueenPosition) ||
            capturePiece(kingPosition, setKingPosition);
    
        // 기물 이동 처리
        if (pieceCaptured || selectedType) {
          if (!isBlackTurn) {
            if (selectedType === 'pawn') movePiece(pawnsPosition, setPawnsPosition, selectedPiece);
            else if (selectedType === 'rook') movePiece(rooksPosition, setRooksPosition, selectedPiece);
            else if (selectedType === 'knight') movePiece(knightsPosition, setKnightsPosition, selectedPiece);
            else if (selectedType === 'bishop') movePiece(bishopsPosition, setBishopsPosition, selectedPiece);
            else if (selectedType === 'queen') movePiece(queenPosition, setQueenPosition, selectedPiece);
            else if (selectedType === 'king') movePiece(kingPosition, setKingPosition, selectedPiece);
          } else {
            if (selectedType === 'pawn') movePiece(blackPawnsPosition, setBlackPawnsPosition, selectedPiece);
            else if (selectedType === 'rook') movePiece(blackRooksPosition, setBlackRooksPosition, selectedPiece);
            else if (selectedType === 'knight') movePiece(blackKnightsPosition, setBlackKnightsPosition, selectedPiece);
            else if (selectedType === 'bishop') movePiece(blackBishopsPosition, setBlackBishopsPosition, selectedPiece);
            else if (selectedType === 'queen') movePiece(blackQueenPosition, setBlackQueenPosition, selectedPiece);
            else if (selectedType === 'king') movePiece(blackKingPosition, setBlackKingPosition, selectedPiece);
          }
        }
      } else {
        // 기물 선택
        const selectPiece = (positions, pieceType, isBlackPiece) => {
          const pieceIndex = positions.findIndex((piece) => piece.row === row && piece.col === col);
          if (pieceIndex !== -1) {
            // 턴에 맞는 기물만 선택할 수 있도록 조건 추가
            if ((isBlackPiece && isBlackTurn) || (!isBlackPiece && !isBlackTurn)) {
              setSelectedPiece(pieceIndex);
              setSelectedType(pieceType);
            }
          }
        };
    
        // 백과 흑의 기물을 모두 보이게 설정 (백 차례에는 백 기물만, 흑 차례에는 흑 기물만 선택 가능)
        selectPiece(pawnsPosition, 'pawn', false);
        selectPiece(rooksPosition, 'rook', false);
        selectPiece(knightsPosition, 'knight', false);
        selectPiece(bishopsPosition, 'bishop', false);
        selectPiece(queenPosition, 'queen', false);
        selectPiece(kingPosition, 'king', false);
        selectPiece(blackPawnsPosition, 'pawn', true);
        selectPiece(blackRooksPosition, 'rook', true);
        selectPiece(blackKnightsPosition, 'knight', true);
        selectPiece(blackBishopsPosition, 'bishop', true);
        selectPiece(blackQueenPosition, 'queen', true);
        selectPiece(blackKingPosition, 'king', true);
      }
    };
  

  const renderPiece = (row, col) => {
    const renderChessPiece = (positions, imagePath) => {
      const piece = positions.find((p) => p.row === row && p.col === col);
      return piece ? (
        <img src={imagePath} alt="piece" className="piece" />
      ) : null;
    };
  
    // 백 기물 렌더링
    return (
      renderChessPiece(pawnsPosition, '/images/white_pawn.png') ||
      renderChessPiece(rooksPosition, '/images/white_rook.png') ||
      renderChessPiece(knightsPosition, '/images/white_knight.png') ||
      renderChessPiece(bishopsPosition, '/images/white_bishop.png') ||
      renderChessPiece(queenPosition, '/images/white_queen.png') ||
      renderChessPiece(kingPosition, '/images/white_king.png') ||
  
      // 흑 기물 렌더링
      renderChessPiece(blackPawnsPosition, '/images/black_pawn.png') ||
      renderChessPiece(blackRooksPosition, '/images/black_rook.png') ||
      renderChessPiece(blackKnightsPosition, '/images/black_knight.png') ||
      renderChessPiece(blackBishopsPosition, '/images/black_bishop.png') ||
      renderChessPiece(blackQueenPosition, '/images/black_queen.png') ||
      renderChessPiece(blackKingPosition, '/images/black_king.png')
    );
  };

  const generateFEN = () => {
    const fenRows = [];
  
    // 1. 기물의 위치를 FEN 형식으로 변환
    for (let row = 8; row > 0; row--) {
      let fenRow = "";
      let emptySquares = 0;
  
      for (let col = 1; col < 9; col++) {
        const piece = getPieceAtPosition(row, col); // 현재 칸에 있는 기물을 가져옴
  
        if (piece) {
          if (emptySquares > 0) {
            fenRow += emptySquares;
            emptySquares = 0;
          }
          fenRow += piece;
        } else {
          emptySquares++;
        }
      }
  
      if (emptySquares > 0) {
        fenRow += emptySquares;
      }
  
      fenRows.push(fenRow);
    }
  
    // 2. 차례 추가 ('w'는 백, 'b'는 흑)
    let fen = fenRows.join('/');
    fen += ` ${isBlackTurn ? 'b' : 'w'}`; // 흑 차례이면 'b', 백 차례이면 'w'
  
    // 3. 캐슬링 가능 여부 추가
    let castlingAvailability = "";
    if (!hasMoved(kingPosition, 'white') && !hasMoved(rooksPosition, 'white')) {
      castlingAvailability += 'K'; // 백 킹 사이드 캐슬링 가능
    }
    if (!hasMoved(kingPosition, 'white') && !hasMoved(rooksPosition, 'white', true)) {
      castlingAvailability += 'Q'; // 백 퀸 사이드 캐슬링 가능
    }
    if (!hasMoved(blackKingPosition, 'black') && !hasMoved(blackRooksPosition, 'black')) {
      castlingAvailability += 'k'; // 흑 킹 사이드 캐슬링 가능
    }
    if (!hasMoved(blackKingPosition, 'black') && !hasMoved(blackRooksPosition, 'black', true)) {
      castlingAvailability += 'q'; // 흑 퀸 사이드 캐슬링 가능
    }
    fen += ` ${castlingAvailability || '-'}`;
  
    // 4. 앙파상 가능 칸 추가 (현재 코드에서는 따로 앙파상 칸을 추적하지 않으므로 생략)
    fen += ` -`;
  
    // 5. 하프무브 카운트 (임시로 0으로 설정)
    fen += ` ${halfmoveClock}`;
  
    // 6. 풀무브 카운트 (흑의 턴이 끝날 때마다 1씩 증가)
    fen += ` ${fullmoveNumber}`;
  
    return fen;
  };
  
  // 기물이 이동했는지 여부를 추적하기 위한 함수
  const hasMoved = (positions, color, isQueenSide = false) => {
    if (color === 'white') {
      return positions.some(piece => piece.row !== 1 || (isQueenSide ? piece.col !== 1 : piece.col !== 8));
    } else {
      return positions.some(piece => piece.row !== 8 || (isQueenSide ? piece.col !== 1 : piece.col !== 8));
    }
  };
  
  // 하프무브 및 풀무브 카운트를 위한 초기 값
  const [halfmoveClock, setHalfmoveClock] = useState(0);
  const [fullmoveNumber, setFullmoveNumber] = useState(1);
  
  // 상태가 변경될 때마다 FEN을 생성하는 로직 수정
  useEffect(() => {
    const fenString = generateFEN();
    console.log(`FEN: ${fenString}`);
  }, [pawnsPosition, rooksPosition, knightsPosition, bishopsPosition, queenPosition, kingPosition,
      blackPawnsPosition, blackRooksPosition, blackKnightsPosition, blackBishopsPosition, blackQueenPosition, blackKingPosition,
      halfmoveClock, fullmoveNumber, isBlackTurn]);
  
  

  
  const getPieceAtPosition = (row, col) => {
    // 백 기물 확인
    if (pawnsPosition.some((p) => p.row === row && p.col === col)) return 'P';
    if (rooksPosition.some((p) => p.row === row && p.col === col)) return 'R';
    if (knightsPosition.some((p) => p.row === row && p.col === col)) return 'N';
    if (bishopsPosition.some((p) => p.row === row && p.col === col)) return 'B';
    if (queenPosition.some((p) => p.row === row && p.col === col)) return 'Q';
    if (kingPosition.some((p) => p.row === row && p.col === col)) return 'K';
  
    // 흑 기물 확인
    if (blackPawnsPosition.some((p) => p.row === row && p.col === col)) return 'p';
    if (blackRooksPosition.some((p) => p.row === row && p.col === col)) return 'r';
    if (blackKnightsPosition.some((p) => p.row === row && p.col === col)) return 'n';
    if (blackBishopsPosition.some((p) => p.row === row && p.col === col)) return 'b';
    if (blackQueenPosition.some((p) => p.row === row && p.col === col)) return 'q';
    if (blackKingPosition.some((p) => p.row === row && p.col === col)) return 'k';
  
    return null;
  };

  

  
  // 8x8 체스판 생성
  const board = [];
  for (let row = 8; row > 0; row--) {
    const rowDivs = [];
    for (let col = 1; col < 9; col++) {
      const isBlack = (row + col -1) % 2 === 1;
  
      

  // 선택된 칸이면 'selected' 클래스를 추가
  const isSelected = selectedSquare.row === row && selectedSquare.col === col;

      rowDivs.push(


        <div
          key={`${row}-${col}`}
          className={`square ${isBlack ? "black" : "white"} ${isSelected ? "selected" : ""}`}
          onClick={() => handleSquareClick(row, col)} // 칸 클릭 시 처리
        >
          {renderPiece(row, col)}
        </div>
      );
    }
    board.push(
      <div key={row} className="row">
        {rowDivs}
      </div>
    );
  }

  return <div className="chess-board">{board}</div>;
};

export default ChessBoard;
