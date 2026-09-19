{
  description = "LaTeX devShell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=26.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    {
      overlays.default = final: prev: {
        qcsp-stem = final.callPackage (
          { stdenv, pkgs, ... }:
          # You can put here the derivation to build your program, for instance:

          stdenv.mkDerivation {
            src = ./.;
            pname = "qcsp-stem";
            version = "unstable";

            nativeBuildInputs = with pkgs; [
              clang
			  cmake
			  doxygen
			  graphviz
			  llvm
              ninja
            ];

            buildInputs = with pkgs; [
              boost
			  catch2
			  clang-tools
			  doxygen
              fftw
              fftwFloat
              matio
			  python3
              uhd
			  zsh
			  zstd
            ];

			doCheck = true;

            cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=ON" ];

          }
        ) { };
      };
    }
    // (flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          system = system;
          overlays = [ self.overlays.default ];
        };
        lib = nixpkgs.lib;
      in
      {
        # Create a new package
        packages = {
          qcsp-stem = pkgs.qcsp-stem;
          default = self.packages.${system}.qcsp-stem; # default program: this way, typing "nix develop" will directly put you in a shell needed to develop the above your program, running "nix build/run" will directly build/run this program etc.
        };
      }
    ));
}
