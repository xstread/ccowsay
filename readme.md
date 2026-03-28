# ccowsay

A C implementation of cowsay. I wanted to write something fun while learning C, and cowsay seemed like a good excuse to mess with text processing and file I/O. It's not trying to replace the original.

## what it does

- Text wrapping (configurable width)
- Speech and thought bubbles (cowsay/cowthink)
- Custom eyes and tongue
- Personality modes (borg, dead, greedy, paranoid, stoned, tired, wired, young)
- COWPATH support
- Reads from stdin or args

## install

```bash
git clone https://github.com/xstread/ccowsay.git
cd ccowsay
make
sudo make install
```

Installs to `/usr/bin`, cow files to `/usr/local/share/ccowsay/cows`.

Uninstall:
```bash
sudo make uninstall
```

## usage

```bash
ccowsay [options] [message]
```

If you don't give it a message, it reads from stdin.

### options

```
-f cowfile    Pick a cow (default: default)
-W width      Wrap width (default: 40)
-e eyes       Custom eyes (2 chars, default: oo)
-T tongue     Custom tongue (2 chars)
-n            No wrapping
-l            List cows
-h            Help
```

### modes

```
-b    Borg (eyes: ==)
-d    Dead (eyes: xx, tongue: U )
-g    Greedy (eyes: $$)
-p    Paranoid (eyes: @@)
-s    Stoned (eyes: **, tongue: U )
-t    Tired (eyes: --)
-w    Wired (eyes: OO)
-y    Young (eyes: ..)
```

### examples

```bash
ccowsay "Hello, World!"
ccowsay -f dragon "I am a dragon"
fortune | ccowsay
cowthink "Hmm, interesting..."
cowthink "How cursed are companies today? Age verification??"
ccowsay -d -W 60 "I'm not feeling so good"
ccowsay -l
```

## custom cows

Cow files use three placeholders:
- `{{eyes}}` - Eye characters
- `{{tongue}}` - Tongue characters  
- `{{thoughts}}` - `\` for speech, `o` for thoughts

Add custom cows to:
- `./cows/` (local)
- `/usr/local/share/ccowsay/cows/` (system)
- Any path in `COWPATH` (colon separated)

```bash
export COWPATH="/path/to/custom/cows:/another/path"
```

## license

See LICENSE file.
